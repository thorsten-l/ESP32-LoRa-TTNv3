from pathlib import Path

configPath = Path(".pio/libdeps/heltec_wifi_lora_32/MCCI LoRaWAN LMIC library/project_config/lmic_project_config.h")

print(" ")
print("**********************************************************************")

if configPath.exists():
  print( "MCCI LoRaWAN LMIC library project config file exists -> overwriting it" )
  print( "Find config in platformio.ini -> build_flags" )
  configFile = open( configPath, 'w')
  print('// project-specific definitions', file = configFile)
  configFile.close()

else:
  print( "Config file is NOT existing" )

print("**********************************************************************")
print(" ")
