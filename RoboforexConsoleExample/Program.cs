using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using RoboforexAPI.Models.Interfaces;
using RoboforexAPI.Services;
using System.Text.Json;


// Create the host builder
var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices((context, services) =>
    {
        // Register a named HttpClient
        services.AddHttpClient("RoboforexHttpClient", client =>
        {
            client.DefaultRequestHeaders.Add("Accept", "application/json");
        });

        // Register the service
        services.AddScoped<IBrokerAPI, RoboforexService>();
    })
    .Build();

var brokerApi = host.Services.GetRequiredService<IBrokerAPI>();
brokerApi.ApiToken = "cefe35a75367d96423db5530d7d9e51dd9737ce95343cca517645be967ab440d";

try
{
    Console.WriteLine("get instruments");
    RoboforexAPI.Models.Entities.InstrumentShort[] arr = await brokerApi.GetInstruments();
    foreach (var item in arr)
    {
        Console.WriteLine(JsonSerializer.Serialize(item));
    }
    Console.WriteLine("Finished");
}
catch(Exception ex)
{
    Console.WriteLine(ex.Message);
}

// Run the host
await host.RunAsync();
