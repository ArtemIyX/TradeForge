using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class AccountCashState
    {
        [JsonPropertyName("my_portfolio")]
        public double MyPortfolio { get; set; } = 0.0;

        [JsonPropertyName("investments")]
        public double Investments { get; set; } = 0.0;

        [JsonPropertyName("available_to_invest")]
        public double AvailableToInvest { get; set; } = 0.0;
    }
}
