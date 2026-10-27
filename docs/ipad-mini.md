# FrostOrtho for iPad mini（初期ロット・PAW3222換装）

## 対象と設計
対象は所有者の初期ロットFrostOrthoとPAW3222換装品。41キー、左エンコーダー、右centralの配線を継承する。
元リポジトリには書き込まず、TFu-26/zmk-config-FrostOrtho-foriPad内の作業ブランチからmainへPRを作成する。

| 項目 | 元FrostOrtho | PrimeOrb Mini / Standard | この版 |
|---|---|---|---|
| ZMK | cormoran v0.3-branch+dya | 公式v0.3-branch | 公式v0.3系 acfd8e5eに固定 |
| PAW3222 | sekigon-gonnoc main | 同ドライバー | df652881に固定 |
| キー数／マトリクス | 41、4行×11列、右offset=6 | Miniは43位置 | FrostOrthoの41位置へ対応付け |
| エンコーダー | EC11、steps=24／triggers=10 | Miniは独自デコーダーと固定wheel、Standardは従来構成 | Mini方式、24 detents／24 triggers |
| エンコーダーA/B | 左D5/D0 | Miniは左D5/D4 | 左D5/D0を維持 |
| ポインティング | PAW3222・1500 CPI・XY反転 | 別のトラックボール配置 | 元FrostOrthoの向き・倍率を維持 |
| キーマップ | Windows向け | Windows/iOS両用 | MiniのiOSをLayer 0、WindowsをLayer 1へ |
| Studio | DYA拡張あり | 公式Studioと一部モジュール | 公式Studio（USB）、DYA固有拡張なし |

公式ZMKへ合わせるため、DYAのBLE管理・設定RPC・センサー動作の実行時変更などは搭載しない。
キーマップは公式StudioでUSB接続して変更できる。sensor割当はソース側の定義を使う。
RGBアダプターと電池診断機能は追加していない。iPad向けの通常BLE入力と電池残量通知は有効。

## Layer 0（PrimeOrb MiniのiOS配列を既定に）
PrimeOrb Miniの `90934dc58b2e635c1cef7f51ff3484866787962a` を基準にした移植。
iOSをLayer 0、WindowsをLayer 1とし、iPadのJIS設定に合わせてiOS側の記号もJISキーコードへ変換する。

```text
Q      W     E      R       T           Y          U       I       O     P/Scroll
A      S     D      F       G           H          J       K       L     -
Shift  Z     X      C       V           B          N       M       ,     .
Ctrl   Cmd   Option Globe   Tab/Symbol  Space/Scroll   Backspace Enter/Arrow Delete Esc/Function Slash/Scroll
```

右最下段は左から **Backspace、Enter、Delete、Esc、Slash**。
Enter長押しはiOS矢印レイヤー6、Esc長押しはFunctionレイヤー2。
P・Space・Slash長押しはiOS数字レイヤー8とスクロールレイヤー9を同時に有効化する。
Tab長押しはiOS記号レイヤー4。Globeは単独キーで、iPad側の言語切替やショートカットに使う。

### 物理位置の対応（所有者確認済み）
- PrimeOrb左のG右隣（source 15）とV右隣（source 27）は、FrostOrthoでは省略。全レイヤーで同じ2位置を省く。
- PrimeOrbのB左隣（source 28）は、FrostOrtho右最下段の左から3番目（position 38）。
- PrimeOrbのH左隣（source 16）は、同じ段の左から4番目（position 39）。
- PrimeOrb右親指のBackspace・Enter・Slashは、FrostOrtho右最下段の1・2・5番目へ。
- コンボの位置もこの対応で変換。詳細は `keymap-mapping.json`。

| Layer | 用途／通常の入口 |
|---|---|
| 0 | iOS既定（JIS） |
| 1 | Windows補助配列。SettingsのZ位置で有効、Shift位置で解除してiOSへ |
| 2 | Function。Esc長押し |
| 3 | Windows記号。Windows時のTab長押し |
| 4 | iOS記号。Tab長押し |
| 5 | Windows矢印。Windows時のEnter長押し |
| 6 | iOS矢印。Enter長押し |
| 7 | Windows数字。Windows時のP/Space/Slash長押し |
| 8 | iOS数字・Globeによるウインドウ操作。P/Space/Slash長押し |
| 9 | トラックボール縦横スクロール。Layer 7/8と同時に有効 |
| 10 | Settings。Escを長押ししたまま、右最下段3番目（通常Delete）を押す |

Settingsは左上Q/W/E/R/TがBT0–4、左ホーム段A/S/D/F/Gが切断操作、
左3段目Shift/ZがiOS/Windows、C/VがUSB/BLE。
右上Y/Uが英数/かな、IがStudio解除。BTプロファイル選択だけではOS配列を変えない。

### マウスとコンボ
PrimeOrb Miniに合わせ、トラックボール操作による自動マウスレイヤーは使わない。
J+Kで左クリック、K+Lで右クリック、J+K+Lで中クリック。
iOS矢印レイヤーでもJ=左クリック、L=右クリックが使える。
Q+W=Esc、U+I+O+P=ウインドウ／タブを閉じる（iOSはCommand+W、WindowsはAlt+F4）。
そのほかの記号コンボもPrimeOrbから物理位置に合わせて移植した。

トラックボールの通常カーソルの向き・2/3倍は前版を維持する。
P/Space/Slash長押し時は縦横スクロールへ変換し、倍率は縦横とも前版の2/3。
PrimeOrbのボール用の配線やCPIはコピーしない。
Globeによるアプリ切替・画面分割等の対応は、iPadOSバージョンやアプリに依存する。

## エンコーダー
所有者が前版の回転方向を実機で確認済み。方向とデコーダーは維持し、
1クリックのホイール量を **±1から±2** に増やした。
全11レイヤーで固定量の縦ホイールを使う。回転速度や押下時間による加速は追加しない。
24クリックの1回転で、片方向の合計は48ホイール単位になる。
トラックボールとは別listenerなので、ボールの倍率やスクロールレイヤーの影響は受けない。

部品表が参照するEC12E2440301は24 detents／24 pulses。
A=D5、B=D0、open/open（11）で4遷移が完了した時だけ1クリックを確定する。
CW上／CCW下のbinding順序は前版と同じ。OSの描画やナチュラルスクロール設定・BLE遅延は別途残る。

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
StandardのPAW3222配線もSCK=P0.05、SDIO=P0.04、CS=P0.09、IRQ=P0.02で一致する。
Mini/Standardのマトリクスや電池ADC設定はコピーしていない。右の物理UARTはD6/D7と行列の競合を避けるため無効化する（USB Studio用CDC UARTとは別）。NFCTピンは両側ともGPIO化。

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
5. エンコーダーをゆっくり24クリック回し48ホイール単位になることをHID記録等で確認。
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
