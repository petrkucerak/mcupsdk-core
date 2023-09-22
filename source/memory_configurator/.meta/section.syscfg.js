let common   = system.getScript("/common");
let book_keeping = []

function loadMemoryRegions () {

 /*  List should include all the MRs defined in this core as well as shared by other cores with this core*/
    let memory_regions = []

    let coreNames =  common.getSysCfgCoreNames();
    let memory_region_module_name = ""
    let region_module = system.modules['/memory_configurator/region'];
    if(region_module !== undefined) {
        if(region_module.$instances[0].mpu_setting)
            memory_region_module_name = "/memory_configurator/memory_region_mpu";
        else
            memory_region_module_name = "/memory_configurator/memory_region";

        let selfCoreName = common.getSelfSysCfgCoreName();
        book_keeping.splice(0, book_keeping.length)
        for (let core of coreNames) {

            let core_module = common.getModuleForCore(memory_region_module_name, core);
            let core_module_instances;
            if(core_module != undefined) {
                core_module_instances = core_module.$instances;
                _.each(core_module_instances, instance => {

                    let obj = { name: " ",
                    opti_share: false
                    }
                    let displayName = instance.$name.concat(" Type: ",instance.type)
                    //displayName = displayName.concat("Opti-share ", instance.opti_share)
                    if (core.includes(selfCoreName)) {
                        memory_regions.push({name: instance.$name, displayName: displayName})
                        obj.name = instance.$name
                        obj.opti_share = instance.opti_share
                    }
                    else if(instance.isShared && instance.shared_cores.includes(selfCoreName)) {
                        memory_regions.push({name: instance.$name, displayName: displayName})
                        obj.name = instance.$name
                        obj.opti_share = instance.opti_share
                    }
                    book_keeping.push(obj)
                })
            }
        }
    }
    else {
        memory_regions.push({name: "", displayName: ""})
    }
    return {memory_regions}
}

function isOptiShare(inst) {

    let memory_arr = book_keeping
    let value = false;

    _.each(memory_arr, item => {
        if( item.name === inst.load_memory || item.name === inst.run_memory) {
            // inst.$uiState.output_sections_opti.hidden = !item.opti_share;
            // inst.$uiState.input_sections_opti.hidden = !item.opti_share;
            // inst.$uiState.output_sections.hidden = item.opti_share;
            // inst.$uiState.input_sections.hidden = item.opti_share;
            // inst.opti_share = item.opti_share
            // return item.opti_share
            value = item.opti_share
    }
    })

    return value;
    // let status = false;

    // if(inst.load_memory.includes("true")) {
    //     status = true;
    // }
    //     // inst.$uiState.output_sections_opti.hidden = !status;
    //     // inst.$uiState.input_sections_opti.hidden = !status;
    //     // inst.$uiState.output_sections.hidden = status;
    //     // inst.$uiState.input_sections.hidden = status;
    // inst.opti_share = status
}

function updateOptiShareConfig(inst) {

    let status = inst.opti_share
    inst.$uiState.output_sections_opti.hidden = !status;
    inst.$uiState.input_sections_opti.hidden = !status;
    inst.$uiState.output_sections.hidden = status;
    inst.$uiState.input_sections.hidden = status;
}


let config = [
    {
        name: "$name",
        isCIdentifier: false
    },
    {
        name: "load_memory",
        displayName: "Load Memory",
        default:"",
        description:'Choose a memory region from the ones added in the Memory regions section.',
        options: () => {return loadMemoryRegions().memory_regions},
        onChange: (inst) => { inst.run_memory = inst.load_memory
                            // inst.$uiState.output_sections_opti.hidden = !inst.opti_share;
                            // inst.$uiState.input_sections_opti.hidden = !inst.opti_share;
                            // inst.$uiState.output_sections.hidden = inst.opti_share;
                            // inst.$uiState.input_sections.hidden = inst.opti_share;
                  }
    },
    {
        name: "run_memory",
        displayName: "Run Memory",
        default: "",
        description:'Choose a memory region from the ones added in the Memory regions section.',
        options: () => {return loadMemoryRegions().memory_regions},
    },
    {
        name: "type",
        displayName: "Type",
        default:"LOAD",
        description:'',
        options: [{ name: "LOAD" }, { name: "DSECT" }, { name: "COPY" }, { name: "NOLOAD" }, { name: "NOINT" }],
    },
    {
        name: "group",
        displayName: "Group Section",
        default: true,
        longDescription:'Check this if all the output sections need to be grouped else uncheck.',
    },
    {
        name: "opti_share",
        displayName: "Opti Share",
        default: false,
        hidden: true,
        description:'',
        getValue: isOptiShare,
    },
]

function validate(inst, report) {

    // if(inst.output_sections.length == 0) {
    //     report.logError("This field can't be kept empty", inst, "output_sections")
    // }
    if(inst.load_memory.length == 0) {
        report.logError("This field can't be kept empty", inst, "load_memory")
    }
    if(inst.run_memory.length == 0) {
        report.logError("This field can't be kept empty", inst, "run_memory")
    }
    if(inst.output_section.length == 0) {
        report.logError("Add atleast 1 output section", inst, "output_section")
    }
}
exports = {
    defaultInstanceName: "CONFIG_SECTION",
	displayName: "Section",
	config: config,
    moduleInstances: addModuleInstances,
    validate: validate
}



function addModuleInstances() {
    let modInstances = new Array();

    modInstances.push({
        name: "output_section",
        displayName: "Output Sections",
        moduleName: "memory_configurator/output_section",
        useArray: true,
        minInstanceCount: 0,
        collapsed: false,
    });

    return modInstances;
}