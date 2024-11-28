

class Application {

    #canvas = null;
    #device = null;
    #context = null;

    constructor() 
    {
        console.log("new application")
        this.#canvas = document.getElementById("window");
    }

    initialize() 
    {
        this.isWebGPUSupportable();

        this.#device = this.createDevice();

        this.createContext();

        console.log("everything ok")
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
        this.#context = canvas.getContext("webgpu");
    }
}



// main


const app = new Application;

app.initialize();

