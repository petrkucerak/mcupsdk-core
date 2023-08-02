let path = require('path');
const _ = require('lodash');

const files = {
    common: [
        "ucman.c",
        "uc_notice.c",
        "simpledb.c",
        "simpledb_ucbind.c",
        "yang_config_init.c",
        "excelfore-tsn-remote_runconf.c",
        "excelfore-tsn-remote.c",
        "yang_modules.c",
        "yang_db_identiyref.c",
        "yang_db_enumeration.c",
        "yang_db_runtime.c",
        "yang_db_access.c",
        "ieee802-dot1q-tsn-config-uni_runconf.c",
        "ieee1588-ptp_runconf.c",
        "ieee802-dot1q-bridge_runconf.c",
        "ieee802-dot1q-tsn-config-uni.c",
        "ieee1588-ptp.c",
        "ieee802-dot1q-bridge.c",
        "ieee1588-ptp_access.c",
        "ieee802-dot1ab-lldp.c",
        "ieee802-dot1ab-lldp_runconf.c",
        "ietf-interfaces_access.c",
        "ietf-interfaces.c",
        "ietf-keychain.c",
        "ietf-keychain_runconf.c",
        "ietf-netconf-server.c",
        "ietf-interfaces_runconf.c",
        "ietf-netconf-server_runconf.c",
        "uc_hwal.c",
        "tsn_data.c",
        "uc_notice_tilld.c",
        "ietf-yang-library.c",
        "ietf-yang-library_runconf.c",
    ],
};

const filedirs = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/hal",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/yangs",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/yangs/generated",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/tilld",
    ],
};

const includes = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/soc/k3/am64x_am243x",
        "${MCU_PLUS_SDK_PATH}/source",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/$(MCU_PLUS_SDK_MCU)/r5f",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack",
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/",
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/core/",
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/core/include",
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/core/include/core",
        "${MCU_PLUS_SDK_PATH}/source/networking/enet/core/include/mod",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/rtos",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/rtos/am243x",
        "${MCU_PLUS_SDK_PATH}/source/networking/tsn/tsn-stack/tsn_uniconf/yangs",
    ],
};

const defines = {
    common: [
        'PRINT_FORMAT_NO_WARNING',
        'CB_ETHERNET_NON_POSIX_H=\\"tsn_combase/tilld/cb_lld_ethernet.h\\"',
        'CB_THREAD_NON_POSIX_H=\\"tsn_combase/tilld/cb_lld_thread.h\\"',
        'CB_EVENT_NON_POSIX_H=\\"tsn_combase/tilld/cb_lld_tmevent.h\\"',
        'CB_IPCSHMEM_NON_POSIX_H=\\"tsn_combase/tilld/cb_lld_ipcshmem.h\\"',
        'COMBASE_NO_INET',
        'COMBASE_NO_XTIMER',
        'COMBASE_NO_CRC',
        'COMBASE_NO_IPCSOCK',
        'UB_LOGCAT=4',
        'UB_LOGTSTYPE=UB_CLOCK_REALTIME',
        'UC_RUNCONF',
    ],
};

const deviceSpecificIncludes = {
    am243x : [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/am243x/r5f",
    ],
    am64x : [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/am64x/r5f",
    ],
    am263x : [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/am263x/r5f",
    ],
    am273x : [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/am273x/r5f",
    ],
    awr294x : [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/awr294x/r5f",
    ],
};


const cflags = {
    common: [
        "-Wno-extra",
        "-Wvisibility",
    ],
    release: [
        "-Oz",
        "-flto",
    ],
};

const deviceSpecific_cflags = {
     am243x : [
        "-mthumb",
        "-fno-strict-aliasing",
    ],
    am64x : [
        "-mthumb",
        "-fno-strict-aliasing",
    ],
    am263x : [
    ],
    am273x : [
        "-fno-strict-aliasing",
    ],
    awr294x : [
        "-fno-strict-aliasing",
    ],
};

const buildOptionCombos = [
    { device: "am263x", cpu: "r5f", cgt: "ti-arm-clang"},
    { device: "am243x", cpu: "r5f", cgt: "ti-arm-clang"},
    { device: "am273x", cpu: "r5f", cgt: "ti-arm-clang"},
    { device: "am64x",  cpu: "r5f", cgt: "ti-arm-clang"},
    { device: "awr294x", cpu: "r5f", cgt: "ti-arm-clang"},
];

function getComponentProperty(device) {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "library";
    property.name = "tsn_uniconf-freertos";
    property.tag  = "tsn_uniconf_freertos";
    property.isInternal = false;

    deviceBuildCombos = []
    for (buildCombo of buildOptionCombos)
    {
        if (buildCombo.device === device)
        {
            deviceBuildCombos.push(buildCombo)
        }
    }
    property.buildOptionCombos = deviceBuildCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.filedirs = filedirs;

    includes.common = _.union(includes.common, deviceSpecificIncludes[device]);
    build_property.includes = includes;

    build_property.defines = defines;

    cflags.common = _.union(cflags.common, deviceSpecific_cflags[device]);
    build_property.cflags = cflags;

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};