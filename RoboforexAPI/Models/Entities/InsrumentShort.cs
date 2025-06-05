using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class InsrumentShort
    {
        [JsonPropertyName("ticker")]
        public string Ticker { get; set; } = string.Empty;

        [JsonPropertyName("description")]
        public string Description { get; set; } = string.Empty;

        [JsonPropertyName("contract_size")]
        public double ContractSize { get; set; } = 0.0;

        [JsonPropertyName("units")]
        public string Units { get; set; } = string.Empty;
    }
}
