let path = require('path');

let device = "am263px";

const files = {
    common: [
        "mmcsd_file_io.c",
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

const includes_r5f = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/fs/freertos_fat/FreeRTOS-FAT/include",
        "${MCU_PLUS_SDK_PATH}/source/fs/freertos_fat/portable",
        "${MCU_PLUS_SDK_PATH}/source/fs/freertos_fat/portable/nortos",
        "${MCU_PLUS_SDK_PATH}/source/fs/freertos_fat/config",
    ],
};

const libdirs = {
    common: [
        "${MCU_PLUS_SDK_PATH}/source/kernel/nortos/lib",
        "${MCU_PLUS_SDK_PATH}/source/drivers/lib",
        "${MCU_PLUS_SDK_PATH}/source/board/pmic/lib",
        "${MCU_PLUS_SDK_PATH}/source/fs/freertos_fat/lib",
    ],
};

const libs_r5f = {
    common: [
        "nortos.am263px.r5f.ti-arm-clang.${ConfigName}.lib",
        "pmic_blackbird.am263px.r5f.ti-arm-clang.${ConfigName}.lib",
        "drivers.am263px.r5f.ti-arm-clang.${ConfigName}.lib",
        "freertos_fat.am263px.r5f.ti-arm-clang.${ConfigName}.lib",
    ],
};

const defines = {
    common: [
    ],
};

const lnkfiles = {
    common: [
        "linker.cmd",
    ]
};

const syscfgfile = "../example.syscfg"

const readmeDoxygenPageTag = "EXAMPLES_DRIVERS_MMCSD_FILE_IO";

const templates_nortos_r5f =
[
    {
        input: ".project/templates/am263px/nortos/main_nortos.c.xdt",
        output: "../main.c",
        options: {
            entryFunction: "mmcsd_file_io_main",
        },
    }
];

const buildOptionCombos = [
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am263px-cc", os: "nortos"},
    { device: device, cpu: "r5fss0-0", cgt: "ti-arm-clang", board: "am263px-lp", os: "nortos"},
];

function getComponentProperty() {
    let property = {};

    property.dirPath = path.resolve(__dirname, "..");
    property.type = "executable";
    property.name = "mmcsd_file_io";
    property.isInternal = false;
    property.buildOptionCombos = buildOptionCombos;

    return property;
}

function getComponentBuildProperty(buildOption) {
    let build_property = {};

    build_property.files = files;
    build_property.filedirs = filedirs;
    build_property.libdirs = libdirs;
    build_property.lnkfiles = lnkfiles;
    build_property.syscfgfile = syscfgfile;
    build_property.readmeDoxygenPageTag = readmeDoxygenPageTag;
    build_property.defines = defines;

    if(buildOption.cpu.match(/r5f*/)) {
        build_property.libs = libs_r5f;
        build_property.templates = templates_nortos_r5f;
        build_property.includes = includes_r5f;
    }

    return build_property;
}

module.exports = {
    getComponentProperty,
    getComponentBuildProperty,
};
