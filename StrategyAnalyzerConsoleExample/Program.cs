// Create the host builder


using CsvHelper;
using CsvHelper.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using RoboforexAPI.Models.Entities;
using RoboforexAPI.Models.Interfaces;
using RoboforexAPI.Services;
using StrategyAnalyzer.Models.Entities;
using StrategyAnalyzer.Models.Interfaces;
using StrategyAnalyzer.Services;
using StrategyAnalyzerConsoleExample;
using System.Diagnostics;
using System.Globalization;
using System.Text.Json;

var host = Host.CreateDefaultBuilder(args)
    .ConfigureServices((context, services) =>
    {
        // Register the service
        services.AddScoped<IBrokerAPI, RoboforexService>();
        services.AddScoped<IStrategyAnalyzer<Deal, AnalyzerConfig>, AnalyzerService>();
        services.AddScoped<IHistoryDataConverter<Deal, QuantXDeal>, QuantXConverter>();
        services.AddHttpClient("RoboforexHttpClient", client =>
        {
            client.DefaultRequestHeaders.Add("Accept", "application/json");
        });
    })
    .Build();

var logger = host.Services.GetRequiredService<ILogger<Program>>();

string quantXPath = @"C:\Users\Wellsaik\Downloads\Tradelist.csv";
List<QuantXDeal> deals = new List<QuantXDeal>();
try
{
    // Configure CsvHelper
    var config = new CsvConfiguration(CultureInfo.InvariantCulture)
    {
        Delimiter = ";", // Adjust if your CSV uses a different delimiter
        HeaderValidated = null, // Disable header validation if needed
        MissingFieldFound = null // Handle missing fields gracefully
    };

    using (var reader = new StreamReader(quantXPath))
    using (var csv = new CsvReader(reader, config))
    {
        csv.Context.RegisterClassMap<QuantXDealMap>();
        // Read CSV records into a list of QuantXDeal objects
        deals = csv.GetRecords<QuantXDeal>().ToList();

        double totalProfit = deals.Sum(x => x.ProfitLoss);
        // Example: Print loaded deals
        logger.LogInformation($"Deals num: {deals.Count}, Total Profit: {totalProfit}");
    }
}
catch (Exception ex)
{
    logger.LogError($"Error reading CSV: {ex.Message}");
}

try
{
    IBrokerAPI roboforexApi = host.Services.GetRequiredService<IBrokerAPI>();
    roboforexApi.ApiToken = "da3da44850eb120e6baa96b532459f31790b055b17a0d82e3fab3a8064ff4875";

    InstrumentShort[] allInstruments = await roboforexApi.GetInstruments();
    InstrumentShort nas100_short = allInstruments.FirstOrDefault(x => x.Ticker == "NAS100") ?? throw new Exception("Failed to retrieve NAS100 short description");
    Account firstAccount = (await roboforexApi.GetAccounts()).First();
    var nas100_full = await roboforexApi.GetInstrumentDescription(new RoboforexAPI.Models.Requests.AccountTickerRequest()
    {
        Ticker = "NAS100",
        AccountId = firstAccount.Id
    });

    logger.LogInformation("Full instrument: {f}, short instrument: {s}",
        JsonSerializer.Serialize(nas100_short),
        JsonSerializer.Serialize(nas100_full));

    IHistoryDataConverter<Deal, QuantXDeal> converter = host.Services.GetRequiredService<IHistoryDataConverter<Deal, QuantXDeal>>();
    ICollection<Deal> convertedDeals = converter.Convert(deals);
    IStrategyAnalyzer<Deal, AnalyzerConfig> analyzer = host.Services.GetRequiredService<IStrategyAnalyzer<Deal, AnalyzerConfig>>();
    analyzer.ImportListOfTrades(convertedDeals);
    analyzer.StrategyConfig = new AnalyzerConfig()
    {
        InstrumentFullDescription = nas100_full,
        InstrumentShortDescription = nas100_short
    };
    var equity = await analyzer.Equity();
    foreach (var item in equity.Profits)
    {
        logger.LogInformation(item.ToString());
    }
}
catch (Exception ex)
{
    logger.LogError($"Error: {ex.Message}");
}


await host.RunAsync();