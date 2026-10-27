# ID の設計

## 実行中の ID: RuntimeID

`core/uuid/RuntimeID.hpp` の `RuntimeID` は8バイトの値型で、同じ実行中だけ有効な識別子を表す。
Window、Window のコールバック、TickFunction、DrawObject に使用する。

- デフォルト構築した値は0であり、`IsValid()` は `false` を返す。
- `RuntimeID::Generate()` で新規採番する。採番元は全利用箇所で共有し、並行呼び出しにも対応する。
- 1から順に採番し、破棄した ID も再利用しない。最大値に到達した後は採番しない。
- `TryGenerate(outID)` は枯渇時に `false` を返し、出力値を変更しない。
  `Generate()` は枯渇を致命的エラーとして扱う。
- `GetValue()` はログや表示用の数値を返す。再起動後や別プロセスとの一意性は保証しない。
- 任意の整数から ID を作る API は提供しない。保存データには `UUID` を使用する。

```cpp
const core::RuntimeID id = core::RuntimeID::Generate();
core::TUnorderedMap<core::RuntimeID, int, core::RuntimeIDHash> values;
values.emplace(id, 42);
```

Tick の依存関係には `GetPrimaryTickFunction()->id` を渡す。
プールの添字からの参照やスロット再利用が必要になった場合は、別途、世代番号を含むハンドルを設計する。

## 保存する ID: UUID

`core/uuid/UUID.hpp` の `UUID` は16バイトの値型で、保存・読み込みをまたいで保持する識別子を表す。

- デフォルト構築は全バイト0の無効値とする。新規生成は `UUID::Generate()` で明示する。
- 新規生成する値は UUIDv4 とし、128bit全体に乱数を取得した後、version と variant を設定する。
- 乱数取得に失敗した場合は致命的エラーとして扱う。
- `IsValid()` は全バイト0でないことを検査する。UUID の version や variant の検査ではない。
- 復元 API は保存された全128bitをそのまま設定する。他の UUID version や無効値も復元できる。
- コピー・ムーブ・復元では新しい ID を採番しない。新しい実体には明示的に `Generate()` した値を割り当てる。

### 文字列での保存・復元

`ToString()` は `00112233-4455-6677-8899-aabbccddeeff` 形式の36文字を返す。
`TrySetString()` は同じ形式の小文字・大文字の16進数を受け付ける。
長さ、区切り、16進数に誤りがある場合は `false` を返し、元の ID を変更しない。

```cpp
const core::UUID original = core::UUID::Generate();
const core::String saved = original.ToString();

// saved をファイルや既存の JSON / Binary アーカイバで String として保存する。
core::UUID restored;
if (!restored.TrySetString(saved))
{
    return false;
}
```

JSON / Binary アーカイバでは `output.Value(saved)` と `input.Value(text)` を使用し、
読み込んだ文字列を `restored.TrySetString(text)` で検証して設定する。

### 16バイトでの保存・復元

`UUID::Bytes` は `TArray<uint8_t, 16>` で、UUID の標準バイト順を表す。
ホストのエンディアンによって順序は変わらない。

```cpp
const core::UUID::Bytes saved = original.GetBytes();
core::UUID restored(saved);

// 既存の UUID へ設定する場合。
restored.SetBytes(saved);
```

`Get()` は同じ16バイトの非所有 `StringView` を返す。NULを含むバイナリデータであり、C文字列ではない。
元の UUID より長く保持する場合はデータをコピーする。
可変長のバイナリデータには `TrySetBytes(view)` を使用する。長さが16以外なら `false` を返し、値を変更しない。

## 一意性とハッシュ

`RuntimeIDHash` / `UUIDHash` は検索用のハッシュ値を提供する。同一性の判定には ID 本体を使用する。
UUID の乱数生成だけでは厳密な一意性は保証できないため、重複を禁止する登録先では ID を比較して登録を検証する。
ロード時に復元した UUID の重複も、登録先の責任で検査する。
コンテナのキーに登録した ID は直接書き換えず、登録を解除してから再登録する。
