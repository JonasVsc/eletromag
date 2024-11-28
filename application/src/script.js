

class Application {

    #canvas = document.getElementById("window");
    #device;
    #context;

    constructor() 
    {
        console.log("new application")
    }

    initialize() 
    {
        this.isWebGPUSupportable();

        this.#device = this.createDevice();

        this.createContext();

    }

    isWebGPUSupportable() 
    {
        if(!navigator.gpu)
            throw new Error("WebGPU not supported in this browser");
    }

    async createDevice() 
    {
        const adapter = await navigator.gpu.requestAdapter();
        if(!adapter)
            throw new Error("No appropriate GPUAdapter found");
        const device = await adapter.requestDevice();
        if(!device)
            throw new Error("Couldn't create device");
        return device;
    }

    async createContext() 
    {
        this.#context = this.#canvas.getContext("webgpu");
    }
}


// main
function main() {
    const app = new Application;
    
    app.initialize();
}

main();

