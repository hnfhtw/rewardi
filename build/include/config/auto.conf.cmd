deps_config := \
	/home/Harald.Netzer/esp/esp-idf/components/app_trace/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/aws_iot/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/bt/Kconfig \
	/home/Harald.Netzer/esp/rewardi/components/cpp_utils/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/driver/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/esp32/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/ethernet/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/fatfs/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/freertos/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/heap/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/libsodium/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/log/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/lwip/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/mbedtls/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/openssl/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/pthread/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/spi_flash/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/spiffs/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/vfs/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/Harald.Netzer/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/Harald.Netzer/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/Harald.Netzer/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/Harald.Netzer/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
