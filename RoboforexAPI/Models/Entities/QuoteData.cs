using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class QuoteData
    {
        [JsonPropertyName("ask_price")]
        public double AskPrice { get; set; } = 0.0;

        [JsonPropertyName("bid_price")]
        public double Bid { get; set; } = 0.0;

        [JsonPropertyName("ask_bid_price_time")]
        public ulong AskBidPriceTime { get; set; } = 0;

        [JsonPropertyName("last_price")]
        public double LastPrice { get; set; } = 0.0;

        [JsonPropertyName("last_price_time")]
        public ulong LastPriceTime { get; set; } = 0;
    }
}
