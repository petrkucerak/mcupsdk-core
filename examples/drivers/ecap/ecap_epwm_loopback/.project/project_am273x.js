let path = require('path');

let device = "am273x";

const files = {
    common: [
        "ecap_epwm_loopback.c",
        "main.c",
    ],
};

/* Relative to where the makefile will be generated
 * Typically at <example_folder>/<BOARD>/<core_os_combo>/<compiler>
 */
const filedirs = {
    common: [
        "..",       /* core_os_combo base */
        "../../..", /* Example base */
    ],
};

const libdirs_nortos = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/kernel/nortos/lib",
        "${MCU_PLUS_SDK_PATH}/source/drivers/lib",
        "${MCU_PLUS_SDK_PATH}/source/board/lib",
    ],
};

const libdirs_freertos = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/lib",
        "${MCU_PLUS_SDK_PATH}/source/drivers/lib",
        "${MCU_PLUS_SDK_PATH}/source/board/lib",
    ],
};

const includes_freertos_r5f = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/FreeRTOS-Kernel/include",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/portable/TI_ARM_CLANG/ARM_CR5F",
        "${MCU_PLUS_SDK_PATH}/source/kernel/freertos/config/am273x/r5f",
    ],
};

const libs_nortos_r5f = {
    common: [
        "nortos.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
    ],
};

const libs_freertos_r5f = {
    common: [
        "freertos.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
        "board.am273x.r5f.ti-arm-clang.${ConfigName}.lib",
    ],
};

const libs_nortos_c66 = {
    common: [
        "nortos.am273x.c66.ti-c6000.${ConfigName}.lib",
        "drivers.am273x.c66.ti-c6000.${ConfigName}.lib",
        "board.am273x.c66.ti-c6000.${ConfigName}.lib",
    ],
};

const lnkfiles = {
    common: [
        "linker.cmd",
    ]
};

const syscfgfile = "../example.syscfg"

const readmeDoxygenPageTag = "EXAMPLES_DRIVERS_ECAP_EPWM_LOOPBACK";

const templates_nortos_r5f =
[
    {
        input: ".project/templates/am273x/nortos/main_nortos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "ecap_epwm_loopback_main",
        },
    }
];

const templates_freertos_r5f =
[
    {
        input: ".project/templates/am273x/freertos/main_freertos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "ecap_epwm_loopback_main",
        },
    }
];

const templates_nortos_c66 =
[
    {
        input: ".project/templates/am273x/nortos/main_nortos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "ecap_epwm_loopback_main",
        },
    },
];

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am273x-evm", os: "nortos"},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am273x-evm", os: "freertos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "ecap_epwm_loopback";
    property.isInternal = false;
    property.description = "An ECAP EPWM Loopback Example. This example captures a square wave with 25% duty cycle."
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.filedirs = filedirs;
	build_property.libdirs = libdirs_nortos;										
    build_property.lnkfiles = lnkfiles;
    build_property.syscfgfile = syscfgfile;
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;

    if(buildOption.cpu.match(/r5f*/)) {
        build_property.libs = libs_nortos_r5f;
        build_property.templates = templates_nortos_r5f;
    }
    if(buildOption.cpu.match(/c66*/)) {
        build_property.libs = libs_nortos_c66;
        build_property.templates = templates_nortos_c66;
    }
    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
