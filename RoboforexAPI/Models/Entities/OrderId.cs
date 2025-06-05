using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class OrderId
    {
        [JsonPropertyName("order_id")]
        public string Id { get; set; } = string.Empty;
    }
}
