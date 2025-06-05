using RoboforexAPI.Models.Responses.Instruments;
using System.Text.Json;

namespace RoboforexConsoleExample
{
    internal class Program
    {
        static void Main(string[] args)
        {
            string jsonString = @"{
              ""code"": ""ok"",
              ""data"": [
                {
                  ""ticker"": ""EURUSD"",
                  ""description"": ""EUR/USD"",
                  ""contract_size"": 1,
                  ""units"": ""Share(s)""
                }
              ]
            }";

            InstrumentsResponse response = JsonSerializer.Deserialize<InstrumentsResponse>(jsonString)!;

            // Access the array of InsrumentShort via the Instruments property
            foreach (var instrument in response.Instruments)
            {
                Console.WriteLine($"Ticker: {instrument.Ticker}, Description: {instrument.Description}, ContractSize: {instrument.ContractSize}, Units: {instrument.Units}");
            }
        }
    }
}
