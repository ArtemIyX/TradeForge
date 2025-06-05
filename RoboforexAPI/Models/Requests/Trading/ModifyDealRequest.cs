using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests.Trading
{
    public class ModifyDealData
    {
        [JsonPropertyName("stop_loss")]
        public double? StopLoss { get; set; } = null;

        [JsonPropertyName("take_profit")]
        public double? TakeProfit { get; set; } = null;
    }

    public class ModifyDealRequest : TradingIdRequest
    {
        public ModifyDealData Body { get; set; } = new ModifyDealData();
    }
}
