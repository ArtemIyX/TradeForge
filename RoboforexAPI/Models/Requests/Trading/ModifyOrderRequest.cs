using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests.Trading
{
    public class ModifyOrderRequestData
    {
        /// <summary>
        /// Limit Price for Limit or Stop Price for Stop.
        /// </summary>
        [JsonPropertyName("price")]
        public double? Price { get; set; } = null;

        /// <summary>
        /// Unix timestamp (UTC) in seconds. 0 value means gtc (good till cancel).
        /// </summary>
        [JsonPropertyName("expiration")]
        public ulong? Expiration { get; set; } = null;

        [JsonPropertyName("stop_loss")]
        public double? StopLoss { get; set; } = null;

        [JsonPropertyName("take_profit")]
        public double? TakeProfit { get; set; } = null;
    }

    public class ModifyOrderRequest : TradingIdRequest
    {
        public ModifyOrderRequestData Body { get; set; } = new ModifyOrderRequestData();
    }
}
