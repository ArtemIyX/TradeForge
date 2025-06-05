using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using RoboforexAPI.Extensions;
using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Enums;
using RoboforexAPI.Models.Interfaces;
using RoboforexAPI.Models.Requests;
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
brokerApi.ApiToken = "";

string accountId = "";
try
{
    var arr = await brokerApi.GetAccounts();
    Console.WriteLine($"Total accounts: {arr.Length}");

    var firstAccount = arr.First();
    Console.WriteLine("First account:" + JsonSerializer.Serialize(firstAccount));
    accountId = firstAccount.Id;

    var accountState = await brokerApi.GetAccountState(new RoboforexAPI.Models.Requests.BaseAccountRequest()
    {
        AccountId = firstAccount.Id
    });
    Console.WriteLine("Cash:" + JsonSerializer.Serialize(accountState.CashState));
    Console.WriteLine("Margin:" + JsonSerializer.Serialize(accountState.MarginState));

    var instruments = await brokerApi.GetInstruments();
    Console.WriteLine($"Total instruments: {instruments.Length}");

    var firstInstrument = instruments.First();
    Console.WriteLine("First instrument: " + JsonSerializer.Serialize(firstInstrument));

    var instrumentData = await brokerApi.GetInstrumentDescription(new RoboforexAPI.Models.Requests.AccountTickerRequest()
    {
        AccountId = firstAccount.Id,
        Ticker = "TSLA"
    });
    Console.WriteLine("TSLA data: " + JsonSerializer.Serialize(instrumentData));

    var quoteData = await brokerApi.GetQuoteData(new RoboforexAPI.Models.Requests.AccountTickerRequest()
    {
        AccountId = firstAccount.Id,
        Ticker = "TSLA"
    });
    Console.WriteLine("TSLA quote: " + JsonSerializer.Serialize(quoteData));

    var orders = await brokerApi.GetOrders(new BaseAccountRequest()
    {
        AccountId = firstAccount.Id
    });
    Console.WriteLine($"Orders ({orders.Length}): " + JsonSerializer.Serialize(orders));
    var deals = await brokerApi.GetDeals(new BaseAccountRequest()
    {
        AccountId = firstAccount.Id
    });
    Console.WriteLine($"Deals ({deals.Length}): " + JsonSerializer.Serialize(deals));
}
catch(Exception ex)
{
    Console.WriteLine(ex.Message);
}

try
{
    var orderPlaced = await brokerApi.PlaceOrder(new RoboforexAPI.Models.Requests.Trading.PlaceOrderRequest()
    {
        AccountId = accountId,
        Body = new RoboforexAPI.Models.Requests.Trading.PlaceOrderRequestData()
        {
            Ticker = "MSFT",
            Side = OrderSide.Buy.ToCustomString(),
            Type = OrderType.Limit.ToCustomString(),
            Volume = 1.0,
            Price = 450.40,
            StopLoss = 437.00
        }
    });

    Console.WriteLine($"Order placed: {JsonSerializer.Serialize(orderPlaced)}");

    await Task.Delay(TimeSpan.FromSeconds(5));

    await brokerApi.ModifyOrder(new RoboforexAPI.Models.Requests.Trading.ModifyOrderRequest()
    {
        AccountId = accountId,
        Id = orderPlaced.Id,
        Body = new RoboforexAPI.Models.Requests.Trading.ModifyOrderRequestData()
        {
            StopLoss = 435.00
        }
    });

    var ordersUpdate = await brokerApi.GetOrders(new BaseAccountRequest()
    {
        AccountId = accountId
    });

    
    Console.WriteLine($"Order modified: {JsonSerializer.Serialize(ordersUpdate)}");

    await Task.Delay(TimeSpan.FromSeconds(5));
    await brokerApi.CancelOrder(new RoboforexAPI.Models.Requests.Trading.CancelOrderRequest()
    {
        AccountId = accountId,
        Id = orderPlaced.Id
    });


    Console.WriteLine($"Order canceled");
}
catch (Exception ex)
{
    Console.WriteLine(ex.Message);
}

// Run the host
await host.RunAsync();
