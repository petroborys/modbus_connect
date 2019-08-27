include $(TOPDIR)/rules.mk

# Name, version and release number
# The name and version of your package are used to define the variable to point to the build directory of your package: $(PKG_BUILD_DIR)
PKG_NAME:=modbus_connect
PKG_VERSION:=0.0.2

TARGET_CFLAGS=-I$(STAGING_DIR)/usr/include/modbus
TARGET_LDFLAGS=-L$(STAGING_DIR)/usr/include/modbus
PKG_BUILD_DEPENDS:=libmodbus libopenssl

# Source settings (i.e. where to find the source codes)
# This is a custom variable, used below
SOURCE_DIR:=./src

include $(INCLUDE_DIR)/package.mk

# Package definition; instructs on how and where our package will appear in the overall configuration menu ('make menuconfig')
define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=ToolSensePackages
  SUBMENU:=Modbus
  TITLE:=modbus_connect
  DEPENDS:=+libmodbus +libmosquitto
endef

# Package description; a more verbose description on what our package does
define Package/$(PKG_NAME)/description
  A simple application to use Modbus.
endef

# Package preparation instructions; create the build directory and copy the source code. 
# The last command is necessary to ensure our preparation instructions remain compatible with the patching system.
define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	cp $(SOURCE_DIR)/* $(PKG_BUILD_DIR)
	$(Build/Patch)
endef

# Package build instructions; invoke the GNU make tool to build our package
define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR) \
		 	CC="$(TARGET_CC)" \
	 	CFLAGS="$(TARGET_CFLAGS)" \
		LDFLAGS="$(TARGET_LDFLAGS)"
endef

# Package install instructions; create a directory inside the package to hold our executable, and then copy the executable we built previously into the folder
define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin
endef

# Script to be run after the package has been installed
define Package/$(PKG_NAME)/postinst
endef

# Script to be run after the package has been installed
define Package/$(PKG_NAME)/prerm
endef

# This command is always the last, it uses the definitions and variables we give above in order to get the job done
$(eval $(call BuildPackage,$(PKG_NAME),+libmosquitto))