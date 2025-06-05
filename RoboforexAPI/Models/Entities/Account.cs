using RoboforexAPI.Extensions;
using RoboforexAPI.Models.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class Account
    {
        [JsonPropertyName("id")]
        public string Id { get; set; } = string.Empty;

        [JsonPropertyName("name")]
        public string Name { get; set; } = string.Empty;

        [JsonPropertyName("type")]
        public string TypeStr { get; set; } = string.Empty;

        [JsonIgnore]
        public AccountType TypeEnum => TypeStr.ToAccountType();

        [JsonPropertyName("currency")]
        public string Currency { get; set; } = string.Empty;

        [JsonPropertyName("status")]
        public string StatusStr { get; set; } = string.Empty;

        [JsonIgnore]
        public AccountStatus StatusEnum => StatusStr.ToAccountStatus();
    }
}
