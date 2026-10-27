"""Hardware invariants for the 41-key FrostOrtho, not PrimeOrb's matrix."""
from pathlib import Path
import re
import unittest

ROOT = Path(__file__).resolve().parents[1]
SHIELD = ROOT / "config/boards/shields/FrostOrtho"

class Configuration(unittest.TestCase):
    def test_layer_sizes_and_sensor_count(self):
        keymap = (ROOT / "config/FrostOrtho.keymap").read_text(encoding="utf-8")
        layers = re.findall(
            r'display-name = "[^"]+";\s*bindings = <(.*?)>;\s*sensor-bindings = <(.*?)>;',
            keymap, re.S)
        self.assertEqual(len(layers), 8)
        for keys, sensors in layers:
            self.assertEqual(len(re.findall(r'&\w+', keys)), 41)
            self.assertEqual(re.findall(r'&\w+', sensors), ["&scroll_up_down"])
        dtsi = (SHIELD / "FrostOrtho.dtsi").read_text(encoding="utf-8")
        coords = re.findall(r'RC\((\d+),(\d+)\)', dtsi)
        self.assertEqual(len(coords), 41)
        self.assertEqual(len(set(coords)), 41)
        self.assertEqual(set(coords), {
            (str(r), str(c)) for r in range(4) for c in range(11)
            if c != 5 or r == 3})
        self.assertEqual(dtsi.count("&key_physical_attrs"), 41)

    def test_frostortho_pins(self):
        dtsi = (SHIELD / "FrostOrtho.dtsi").read_text(encoding="utf-8")
        left = (SHIELD / "FrostOrtho_L.overlay").read_text(encoding="utf-8")
        right = (SHIELD / "FrostOrtho_R.overlay").read_text(encoding="utf-8")
        self.assertIn('a-gpios = <&xiao_d 5 ', dtsi)
        self.assertIn('b-gpios = <&xiao_d 0 ', dtsi)
        self.assertIn('detents-per-rotation = <24>', dtsi)
        self.assertIn('triggers-per-rotation = <24>', dtsi)
        def gpio_pins(text):
            return re.findall(r'<&(xiao_d|gpio[01])\s+(\d+)\s+GPIO_ACTIVE_HIGH>', text)
        self.assertEqual(gpio_pins(left), [
            ("xiao_d","10"),("xiao_d","9"),("xiao_d","8"),
            ("xiao_d","7"),("gpio0","10"),("gpio0","9")])
        self.assertEqual(gpio_pins(right), gpio_pins(left)[:-1])
        self.assertIn('col-offset = <6>', right)
        self.assertIn('cs-gpios = <&gpio0 9 GPIO_ACTIVE_LOW>', right)
        self.assertIn('irq-gpios = <&gpio0 2 ', right)
        self.assertEqual(re.findall(r'NRF_PSEL\(SPIM_(\w+), (\d+), (\d+)\)', right),
                         [("SCK","0","5"),("MOSI","0","4"),("MISO","0","4")]*2)
        self.assertIn('compatible = "pixart,paw3222"', right)
        self.assertIn('res-cpi = <1500>', right)

    def test_no_removed_module_dependencies(self):
        paths = list((ROOT / "config").rglob("*.overlay"))
        paths += list((ROOT / "config").rglob("*.keymap"))
        paths += list((ROOT / "config").rglob("*.conf"))
        for p in paths:
            text = p.read_text(encoding="utf-8")
            self.assertNotRegex(text, r'runtime-input-processor|battery_history_request|rsr_')
            self.assertNotRegex(text, r'CONFIG_ZMK_(BLE_MANAGEMENT|SETTINGS_RPC|SPLIT_RELAY_EVENT|RUNTIME_)')
        keymap = (ROOT / "config/FrostOrtho.keymap").read_text(encoding="utf-8")
        self.assertNotIn("&msc", keymap)
        self.assertIn("tap-ms = <0>", keymap)

if __name__ == "__main__":
    unittest.main()
