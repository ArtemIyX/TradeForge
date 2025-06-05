using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests.Trading
{
    public class PlaceOrderRequestData : ModifyOrderRequestData
    {
        [JsonPropertyName("ticker")]
        public string Ticker { get; set; } = string.Empty;

        [JsonPropertyName("side")]
        public string Side { get; set; } = string.Empty;


        [JsonPropertyName("type")]
        public string Type { get; set; } = string.Empty;

        [JsonPropertyName("volume")]
        public double Volume { get; set; } = 0;
    }

    public class PlaceOrderRequest : BaseAccountRequest
    {
        public PlaceOrderRequestData Body { get; set; } = new PlaceOrderRequestData();
    }
}
