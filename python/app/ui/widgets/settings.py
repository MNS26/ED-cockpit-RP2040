''' app/ui/widgets/treeview.py '''
import os
from ...utils.config import AppConfig
from .devicelist import DeviceList
#from pathlib import Path
from PyQt6.QtCore import Qt, pyqtSignal
from PyQt6.QtWidgets import  QLabel, QWidget, QPushButton, QFileDialog, QGridLayout, QLineEdit, QTabWidget, QCheckBox, QHBoxLayout, QVBoxLayout


class Settings(QWidget):
  # Report name change
  deviceNameChanged = pyqtSignal(str)
  # Report device
  device = pyqtSignal(dict)
  """
  This "window" is a QWidget. If it has no parent, it
  will appear as a free-floating window as we want.
  """
  def __init__(self):

    # Initialize settings window
    super(Settings, self).__init__()
    self.setWindowTitle("Settings")
    self.setGeometry(0, 0,  650, 500)
    self.AppConfig = AppConfig()
    self.SavedDevice = self.AppConfig.usb_device
    self.devicelist = DeviceList(self,self.SavedDevice)
    self.directory = QLineEdit()
    self.log_checkbox = QCheckBox()
    self.usb_checkbox = QCheckBox()

    self.devicelist.activated.connect(self.update_userdata)
    self.log_checkbox.clicked.connect(lambda x: self.log_click_event(x))
    self.usb_checkbox.clicked.connect(lambda x: self.usb_click_event(x))
  
    # Setup Grid
    main_layout = QGridLayout()
    self.setLayout(main_layout)
    
    # Create tab widget
    tab = QTabWidget(self)

    # Add tabds to page
    tab.addTab(self.usb_device(),"USB")
    tab.addTab(self.journal(),"Journal")
    #tab.addTab(coriolis_page, "Coriolis")
    #tab.addTab(eddn_page, "EDDN")
    #tab.addTab(edsm_page, "EDSM")
    #tab.addTab(inara_page, "Inara")
    main_layout.addWidget(tab,0,0)

  def show(self) -> None:
    self.log_checkbox.setChecked(AppConfig.enable_usb)
    self.usb_checkbox.setChecked(AppConfig.enable_log)
    return super().show()

  

  def getCurrentDevice(self):
    self.devices = self.devicelist.listDevices()
    # if We already have it we dont have to go through settings to get it
    currentDevice = None
    for device_info in self.devices:
      device_name = f"{device_info['manufacturer_string']} - {device_info['product_string']}"
      if self.SavedDevice == device_name.rstrip():
        currentDevice = device_info
    if currentDevice:
      self.device.emit(currentDevice)
    else:
      self.device.emit({})

  
  def usb_device(self) -> QWidget:
    # USB tab

    toggle = QHBoxLayout()    
    toggle.addWidget(self.usb_checkbox)
    toggle.addWidget(QLabel('Enable USB'))
    toggle.setAlignment(Qt.AlignmentFlag.AlignLeft)
    
    list = QVBoxLayout()
    list.addWidget(self.devicelist)

    pageLayout = QVBoxLayout()
    pageLayout.addLayout(toggle)
    pageLayout.addLayout(list)
    pageLayout.setAlignment(Qt.AlignmentFlag.AlignTop)

    usb_page = QWidget(self)
    usb_page.setLayout(pageLayout)
    return usb_page


  # Journal tab
  def journal(self) -> QWidget:
    self.directory.setText(self.AppConfig.game_log_dir)

    file_browser_btn = QPushButton('Browse')
    file_browser_btn.clicked.connect(lambda: self.open_directory_dialog('Elite Dangerous Logs'))

    toggle = QHBoxLayout()    
    toggle.addWidget(self.log_checkbox)
    toggle.addWidget(QLabel('Enable Journal'))
    toggle.setAlignment(Qt.AlignmentFlag.AlignLeft)

    logFile = QHBoxLayout()
    logFile.addWidget(QLabel('Directory:'))
    logFile.addWidget(self.directory)
    logFile.addWidget(file_browser_btn)
    logFile.setEnabled(False)

    pageLayout = QVBoxLayout()
    pageLayout.addLayout(toggle)
    pageLayout.addLayout(logFile)
    pageLayout.setAlignment(Qt.AlignmentFlag.AlignTop)

    journal_page = QWidget(self)
    journal_page.setLayout(pageLayout)
    return journal_page
  
  def coriolis(self) -> QWidget:
    # Coriolis tab
    coriolis_page = QWidget(self)
    layout = QGridLayout()
    coriolis_page.setLayout(layout)
    return coriolis_page
  
  def eddn(self) -> QWidget:
    # EDDN tab
    eddn_page = QWidget(self)
    layout = QGridLayout()
    eddn_page.setLayout(layout)
    return eddn_page
  
  def edsm(self) -> QWidget:
    # EDSM tab
    edsm_page = QWidget(self)
    layout = QGridLayout()
    edsm_page.setLayout(layout)
    return edsm_page
  
  def inara(self) -> QWidget:
    # Inara tab
    inara_page = QWidget(self)
    layout = QGridLayout()
    inara_page.setLayout(layout)
    return inara_page



  def log_click_event(self, click) -> None:
    AppConfig.enable_log = click
    print(click)
    pass

  def usb_click_event(self, click) -> None:
    AppConfig.enable_usb = click
    print(click)
    pass


  def update_userdata(self,index):
#    if index>= 0:
      selected_index = self.devicelist.currentIndex()
      user_data = self.devicelist.itemData(selected_index) # Replace with the actual new data

      #print(self.devicelist.itemData(selected_index))
      #self.devicelist.setItemData(selected_index, user_data)
      print(f"{user_data['manufacturer_string']} - {user_data['product_string']}")
      self.deviceNameChanged.emit(f"{user_data['manufacturer_string']} - {user_data['product_string']}")
      self.device.emit(user_data)


  def open_directory_dialog(self,section=None ) -> None:

    # Open the file dialog in the specified directory
    initial_directory = str(os.path.expanduser('~')) if self.AppConfig.game_log_dir == "" else self.AppConfig.game_log_dir
    selected_directory = QFileDialog.getExistingDirectory(self, "Select Journal directory", initial_directory)

    if selected_directory:
      self.directory.setText(selected_directory)
      self.AppConfig.game_log_dir = selected_directory
      self.AppConfig.save_setting(section, 'directory', selected_directory)  
