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
    public class Deal
    {
        [JsonPropertyName("id")]
        public string Id { get; set; } = string.Empty;

        [JsonPropertyName("ticker")]
        public string Ticker { get; set; } = string.Empty;

        [JsonPropertyName("volume")]
        public double Volume { get; set; } = 0.0;

        [JsonPropertyName("side")]
        public string SideStr { get; set; } = string.Empty;
        public OrderSide SideEnum => SideStr.ToOrderSide();

        [JsonPropertyName("open_price")]
        public double OpenPrice { get; set; } = 0.0;

        [JsonPropertyName("open_time")]
        public ulong OpenTime { get; set; } = 0;

        [JsonPropertyName("close_price")]
        public double? ClosePrice { get; set; } = null;

        [JsonPropertyName("close_time")]
        public ulong? CloseTime { get; set; } = null;

        [JsonPropertyName("status")]
        public string StatusStr { get;set; } = string.Empty;
        public DealStatus StatusEnum => StatusStr.ToDealStatus();
    }
}
