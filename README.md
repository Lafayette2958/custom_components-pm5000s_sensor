i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true
  frequency: 100kHz

external_components:
  - source: local
    path: custom_components/pm5000s_sensor

sensor:
  - platform: pm5000s_sensor
    pm_0_3:
      name: "Particles >0.3μm"
    pm_0_5:
      name: "Particles >0.5μm"
    pm_1_0:
      name: "Particles >1.0μm"
    pm_2_5:
      name: "Particles >2.5μm"
    pm_5_0:
      name: "Particles >5.0μm"
    pm_10:
      name: "Particles >10μm"
