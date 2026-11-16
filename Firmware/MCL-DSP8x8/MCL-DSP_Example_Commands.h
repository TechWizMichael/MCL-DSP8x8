
void setup_custom_dsp() {
  String custom_commands[] = {
    // Muting mixer for custom routing
    "MIX/CH1/CH1/-144",
    "MIX/CH2/CH2/-144",
    "MIX/CH3/CH3/-144",
    "MIX/CH4/CH4/-144",
    "MIX/CH5/CH5/-144",
    "MIX/CH6/CH6/-144",
    "MIX/CH7/ch7/-144",
    "MIX/CH7/CH7/-144",
    "MIX/CH8/CH8/-144",

    // Left channel routing
    "MIX/CH1/CH1/0",
    "MIX/CH1/CH2/0",
    "MIX/CH1/CH7/0",

    // Right channel routing
    "MIX/CH2/CH3/0",
    "MIX/CH2/CH4/0",
    "MIX/CH2/CH7/0",

    // Center channel routing
    "MIX/CH3/CH1/0",
    "MIX/CH3/CH2/0",
    "MIX/CH3/CH3/0",
    "MIX/CH3/CH4/0",
    "MIX/CH3/CH7/0",

    // Sub channel routing
    "MIX/CH4/CH1/0",
    "MIX/CH4/CH3/0",
    "MIX/CH4/CH7/0",
    
    // Channel 1 (Left LF) Settings
    "CH1/EQ1/TYPE/4",
    "CH1/EQ1/FREQ/150",
    "CH1/EQ1/BOOST/8",
    "CH1/EQ1/EN",
    "CH1/LP1/FREQ/2500",
    "CH1/LP1/EN",
    "CH1/LP2/FREQ/2500",
    "CH1/LP2/EN",
    "CH1/HP1/FREQ/100",
    "CH1/HP1/EN",
    "CH1/HP2/FREQ/100",
    "CH1/HP2/EN",
    "CH1/EN",

    // Channel 3 (Right LF) Settings
    "CH3/EQ1/TYPE/4",
    "CH3/EQ1/FREQ/150",
    "CH3/EQ1/BOOST/8",
    "CH3/EQ1/EN",
    "CH3/LP1/FREQ/2500",
    "CH3/LP1/EN",
    "CH3/LP2/FREQ/2500",
    "CH3/LP2/EN",
    "CH3/HP1/FREQ/100",
    "CH3/HP1/EN",
    "CH3/HP2/FREQ/100",
    "CH3/HP2/EN",
    "CH3/EN",

    // Channel 2 (Left HF) Settings
    "CH2/EQ1/TYPE/3",
    "CH2/EQ1/FREQ/16000",
    "CH2/EQ1/BOOST/6",
    "CH2/EQ1/Q/5",
    "CH2/EQ1/EN",
    "CH2/HP1/FREQ/2500",
    "CH2/HP1/EN",
    "CH2/HP2/FREQ/2500",
    "CH2/HP2/EN",
    "CH2/GAIN/-12",
    "CH2/EN",

    // Channel 4 (Right HF) Settings
    "CH4/EQ1/TYPE/3",
    "CH4/EQ1/FREQ/16000",
    "CH4/EQ1/BOOST/6",
    "CH4/EQ1/Q/5",
    "CH4/EQ1/EN",
    "CH4/HP1/FREQ/2500",
    "CH4/HP1/EN",
    "CH4/HP2/FREQ/2500",
    "CH4/HP2/EN",
    "CH4/GAIN/-12",
    "CH4/EN",

    // Channel 4 (Right HF) Settings
    "CH7/LP1/FREQ/100",
    "CH7/LP1/Q/0.54",
    "CH7/LP1/EN",
    "CH7/LP2/FREQ/100",
    "CH7/LP2/Q/1.31",
    "CH7/LP2/EN",
    "CH7/GAIN/6",
    "CH7/EN",

    "STOP"
  };

  int i = 0;
  String temp_command = custom_commands[i];

  while(!(temp_command.equals("STOP"))) {
    parse_command(temp_command);
    i = i + 1;
    temp_command = custom_commands[i];
  }
}