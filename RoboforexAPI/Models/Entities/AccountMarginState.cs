using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class AccountMarginState
    {
        [JsonPropertyName("balance")]
        public double Balance { get; set; } = 0.0;

        [JsonPropertyName("unrealized_pl")]
        public double UnrealizedPnl { get;set; } = 0.0;

        [JsonPropertyName("equity")]
        public double Equity { get; set; } = 0.0;

        [JsonPropertyName("margin")]
        public double Margin { get; set; } = 0.0;

        [JsonPropertyName("free_margin")]
        public double FreeMargin { get; set; } = 0.0;
    }
}
