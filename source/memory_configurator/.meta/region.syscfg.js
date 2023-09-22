let config = [
    {
        name: "$name",
        hidden: false,
        isCIdentifier: false
    },
    {
        name: "mpu_setting",
        displayName: "Automate MPU Setting",
        default: false,
        longDescription: 'Checking this would save the user from manually configure MPU for regions.\
        Uncheck this to get flexibility in configuring MPU.'
    },
]

function addModuleInstances(inst) {
    let modInstances = new Array();
    let module_name = ""

    if (inst.mpu_setting) {
        module_name = "memory_configurator/memory_region_mpu"
    }
    else {
        module_name = "memory_configurator/memory_region"
    }

    modInstances.push({
        name: "memory_region",
        displayName: "Region",
        moduleName: module_name,
        useArray: true,
        minInstanceCount: 0,
        collapsed: false,
    });

    return modInstances;
}

exports = {
    defaultInstanceName: "MEMORY_REGION_CONFIGURATION",
	displayName: "Memory Region",
	config: config,
    maxInstances: 1,
    moduleInstances: addModuleInstances,
}