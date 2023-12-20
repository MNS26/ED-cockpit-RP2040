''' app/utils/config.py '''

import os
import configparser
class AppConfig:
  """
  Configuration File
  """
  APP_NAME: str = "PyQt template"
  # Default is users home directory
  enable_log: bool = False
  enable_usb: bool = False
  game_log_dir = ''
  usb_device = ''
  def __init__(self) -> None:
    self.load_config()

  # Get the directory of the current script
  script_dir = os.path.dirname(os.path.abspath(__file__))

  # Go up two levels to the main directory
  main_dir = os.path.abspath(os.path.join(script_dir, '..', '..'))

  # Construct the path to the config file
  config_file_path = os.path.join(main_dir, 'config.cfg')

  @classmethod
  def initialize(cls) -> None:
    """
    Perform any necessary initializations here, e.g.:
    - Loading settings from a file
    """        
    cls.load_config()

  @classmethod
  def load_config(cls) -> None:
    """
    Load settings from the config file and update class variables.
    """
    config_file_path = AppConfig.config_file_path

    if not os.path.exists(config_file_path):
      return  # Config file doesn't exist, nothing to load

    config = configparser.ConfigParser()
    config.read(config_file_path)

    if 'Elite Dangerous Logs' in config and 'Enabled' in config['Elite Dangerous Logs']:
      if 'True' == config['Elite Dangerous Logs']['enabled']:
        AppConfig.enable_log = True

    if 'Elite Dangerous Logs' in config and 'Directory' in config['Elite Dangerous Logs']:
      if os.path.exists(config['Elite Dangerous Logs']['Directory']):
        AppConfig.game_log_dir = config['Elite Dangerous Logs']['Directory']


    if 'USB device' in config and 'enabled' in config['USB device']:
      if 'True' == config['USB device']['enabled']:
        AppConfig.enable_usb = True

    if 'USB device' in config and 'Device' in config['USB device']:
      AppConfig.usb_device = config['USB device']['device']

  def save_setting(self, section, parameter, value):
    config = configparser.ConfigParser()

    # Read the existing file if it exists
    try:
      config.read(AppConfig.config_file_path)
    except FileNotFoundError:
      pass  # No need to worry if the file doesn't exist yet

    # If the section doesn't exist, add it
    if section not in config:
      config.add_section(section)

    # Set the parameter's value
    config.set(section, parameter, value)

    # Write the updated configuration back to the file
    with open(AppConfig.config_file_path, 'w') as config_file:
      config.write(config_file)


  def read_config(self, section, parameter) -> str:
    config = configparser.ConfigParser()

    try:
      config.read(AppConfig.config_file_path)
    except FileNotFoundError:
      return str(None)  # File doesn't exist yet, return None

    # If the section doesn't exist or parameter not found, return None
    if section not in config or parameter not in config[section]:
      return str(None)

    # Return the parameter's value
    return config[section][parameter]