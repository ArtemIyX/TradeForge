using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses
{
    public class BaseResponse
    {
        [JsonPropertyName("code")]
        public string Code { get; set; } = string.Empty;

        [JsonPropertyName("data")]
        public object? Data { get; set; } = null;

        [JsonPropertyName("msg")]
        public string? ErrorMsg { get; set; } = null;

        public bool Error => Code.Equals("error");
    }
}
