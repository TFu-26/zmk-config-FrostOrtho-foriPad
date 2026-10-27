# FrostOrtho for iPad mini（初期ロット・PAW3222換装）

## 対象と設計
対象は所有者の初期ロットFrostOrthoとPAW3222換装品。41キー、左エンコーダー、右centralの配線を継承する。
元リポジトリには書き込まず、TFu-26/zmk-config-FrostOrtho-foriPad内の作業ブランチからmainへPRを作成する。

| 項目 | 元FrostOrtho | PrimeOrb Mini / Standard | この版 |
|---|---|---|---|
| ZMK | cormoran v0.3-branch+dya | 公式v0.3-branch | 公式v0.3系 acfd8e5eに固定 |
| PAW3222 | sekigon-gonnoc main | 同ドライバー | df652881に固定 |
| キー数／マトリクス | 41、4行×11列、右offset=6 | 別の物理配置 | FrostOrthoの41位置を維持 |
| エンコーダー | EC11、steps=24／triggers=10 | Miniは独自デコーダーと固定wheel、Standardは従来構成 | Mini方式、24 detents／24 triggers |
| エンコーダーA/B | 左D5/D0 | Miniは左D5/D4 | 左D5/D0を維持 |
| ポインティング | PAW3222・1500 CPI・XY反転 | 別のトラックボール配置 | 元FrostOrthoの向き・倍率を維持 |
| 編集 | Windows向けControlショートカット | Windows/iOS両用 | iPad向けCommandショートカット |
| Studio | DYA拡張あり | 公式Studioと一部モジュール | 公式Studio（USB）、DYA固有拡張なし |

公式ZMKへ合わせるため、DYAのBLE管理・設定RPC・センサー動作の実行時変更などは搭載しない。
キーマップは公式StudioでUSB接続して変更できる。sensor割当はソース側の定義を使う。
RGBアダプターと電池診断機能は追加していない。iPad向けの通常BLE入力と電池残量通知は有効。

## Layer 0（JIS）
上から3行はQWERTY。左右の間にある余白は物理的な空間。
表の「tap / hold」は短押し／長押し。CommandはUSB HIDのGUIキー、OptionはAltキー。

```text
Q       W       E       R          T             Y       U       I       O       P
A       S       D       F          G             H       J       K       L       - / L5
Z/Shift X       C       V          B             N       M       ,       .       / / L7
Tab/Ctrl Cmd    Option  英数/L7    Space/L2  かな/L3  Enter/L1 Backspace Globe Esc/L4  ろ/Shift
```

英数=LANG2、かな=LANG1、Globeは専用Consumer usage。
iPadのハードウェアキーボードを日本語（JIS）に設定する。
英数／かなの処理はiPad側の入力ソースにも依存するので実機で確認する。
Globeも直接押せるため、言語切替やOS側ショートカットに使える。

| Layer | 入口 | 主な用途 |
|---|---|---|
| 0 | 起動時 | iPad JIS入力、Command/Option、英数/かな |
| 1 | Enter長押し | F1–F12、音量・再生、Delete |
| 2 | Space長押し | 数字・JIS記号（@、:、括弧、円、縦棒、アンダーバー等） |
| 3 | かな長押し | 矢印、Command+矢印、PageUp/Down、Delete。右端ホーム段でL6 |
| 4 | Esc長押し | Y/U/I/O/PでBTプロファイル0–4、H=BLE、J=USB、N=Studio解除、/=現在プロファイル削除 |
| 5 | - 長押し | トラックボール縦スクロール |
| 6 | トラックボール操作／L3右端ホーム段長押し | U=左クリック、I=右クリック、O=中クリック |
| 7 | 英数または/長押し | Command編集操作とトラックボール横スクロール |

L7はQ=Undo、W=Redo、E=検索、R=更新、T=新規タブ、Y=Globe+H、
U=Command+Space、I=Command+Tab、O=Command+W、
A=全選択、S=保存、X=切り取り、C=コピー、V=貼り付け、B=Command+Shift+3。
アプリやiPadOSバージョンによりショートカットの対応が異なる。

オートマウスはキー入力後400msの待機を設け、最後の移動／クリックから1秒で解除する。
U/I/O・修飾キーはマウス操作を継続する。元版の100秒タイムアウトを1秒へ短縮した。
レイヤー5/7ではトラックボールをスクロールに使い、エンコーダーは全レイヤーで固定量の縦ホイールとなる。

## エンコーダー
部品表が参照する製品はEC12E2440301。メーカー仕様は24 detents／24 pulses。
所有者も24クリックと確認済み。Miniの4遷移デコーダーを移植し、
FrostOrthoのA=D5、B=D0へ接続する。open/open（11）で1周期が完了した時だけ1出力。
不正遷移や途中での反転を抑え、時間ベースの加速や方向ロックを追加しない。
`enc_whl`は1回の押下でWHEELを±1だけ送り、解放は何もしない。
CW=上、CCW=下を初期設定とし、取付方向が逆なら`scroll_up_down.bindings`の順序だけ交換する。
トラックボールとは別のlistenerを使うため、ボールの倍率・レイヤー・自動マウスの影響を受けない。
OSの描画・スクロール方向設定やBLEの遅延は別途残る。

## 配線監査（ソース上）

初期ロット用タグ v1.0.0（085e86a78539f855cc32dada44792ec357178694）とも比較済み。
共通dtsi（41位置・行列・エンコーダー配線）と左右の列ピン・PAW換装前のSPI/IRQピンは現mainと一致する。
v1.0.0のPMW3610向け設定には換装ガイドのPAW3222設定を適用している。

初期ロット現物の導通・改造状態・RCフィルター実装は未確認。以下は元版のDevicetreeと換装ガイドを基準とする。
左右で同じGPIO番号を使用していても、別MCUなので競合ではない。

| 機能 | 左 | 右 |
|---|---|---|
| rows | D1,D2,D3,D6、active high/pull-down | 同左 |
| cols | D10,D9,D8,D7,P0.10,P0.09 | D10,D9,D8,D7,P0.10 |
| diode | col2row | col2row |
| encoder | A=D5/P0.05、B=D0/P0.02、pull-up | なし |
| PAW SCK | なし | P0.05 |
| PAW SDIO | なし | P0.04（MOSI/MISO共有） |
| PAW CS | なし | P0.09、active low |
| PAW IRQ | なし | P0.02、active low/pull-up |
| sensor | なし | PAW3222、2MHz、1500 CPI |
| BLE役割 | peripheral | central |

PAW3222は換装ガイド通り、XY反転・2/3倍のカーソル倍率を継承。
PrimeOrbのSPIピンや電池ADC設定はコピーしていない。NFCTピンは両側ともGPIO化。

## ビルドと書き込み
GitHub Actionsは設定検証、エンコーダーのC++テスト（ASan/UBSan）後、
右・左・settings_resetをビルドする。成功成果物の`firmware.zip`にある同じビルドの左右UF2を使用する。
リポジトリに元からある`firmware/`のUF2は今回の版ではない。

1. 初回は現在の設定を控え、右と左の新しいUF2をそれぞれ書き込む。
2. DYAから公式ZMKへ移るため、保存済みStudioキーマップ・左右の接続情報が残っていると、
   ソースと違う配列になったり接続できなかったりする可能性がある。
   その場合は両側にsettings_resetを書いた後、それぞれ対応UF2を書き直す。
   settings_resetは保存設定とペアリングを消すので、iPad側でも旧登録を解除して再ペアリングする。
3. BT0をiPad mini用としてペアリング。数字・JIS記号・英数/かな・Command・Globeを確認する。
4. U/I/Oクリック、カーソル向き、縦横スクロール、スリープ復帰、電池残量表示を確認する。
5. エンコーダーをゆっくり24クリック回し24ホイール単位になることをHID記録等で確認。
   両方向・即時反転・速い回転・クリック付近の揺らしをUSB/BLE両方で確認する。

合成信号テスト・ビルド成功だけでは実機のノイズ、クリック位相、取付方向、iPadの操作感は確定しない。
PRはビルド成功後にこのForkのmain宛てに作成する。自動マージや実機書き込みは行わない。

## 参考
- [元FrostOrtho](https://github.com/imo00o/zmk-config-FrostOrtho)
- [PAW3222換装ガイド](https://github.com/imo00o/zmk-config-FrostOrtho/blob/main/doc/センサーモジュール付け替えガイド.md)
- [部品表が参照するエンコーダー](https://shop.yushakobo.jp/products/2141)
- [EC12E2440301メーカー仕様](https://tech.alpsalpine.com/e/products/detail/EC12E2440301/)
- [PrimeOrb Mini固定スクロール実装](https://github.com/TFu-26/zmk-config-PrimeOrb-Mini/blob/main/docs/encoder-scroll.md)
- [Appleのキーボードショートカット案内](https://support.apple.com/ja-jp/guide/ipad/ipaddf61a0c2/ipados)
