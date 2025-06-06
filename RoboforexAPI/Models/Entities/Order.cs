﻿using RoboforexAPI.Extensions;
using RoboforexAPI.Models.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;
using static System.Runtime.InteropServices.JavaScript.JSType;

namespace RoboforexAPI.Models.Entities
{
    public class Order
    {
        [JsonPropertyName("id")]
        public string Id { get; set; } = string.Empty;

        [JsonPropertyName("ticker")]
        public string Ticker { get; set; } = string.Empty;

        [JsonPropertyName("volume")]
        public double Volume { get; set; } = 0.0;

        [JsonPropertyName("side")]
        public string SideStr { get; set; } = string.Empty;

        [JsonIgnore]
        public OrderSide SideEnum => SideStr.ToOrderSide();

        [JsonPropertyName("type")]
        public string TypeStr { get; set; } = string.Empty;

        [JsonIgnore]
        public OrderType TypeEnum => TypeStr.ToOrderType();

        [JsonPropertyName("filled_price")]
        public double? FilledPrice { get; set; } = null;

        [JsonPropertyName("price")]
        public double? Price { get; set; } = null;

        [JsonPropertyName("expiration")]
        public ulong? Expiration { get; set; } = null;

        [JsonPropertyName("last_modified")]
        public ulong? LastModified { get; set; } = null;

        [JsonPropertyName("comment")]
        public string? Comment { get; set; } = null;

        [JsonPropertyName("deals")]
        public object? Deals { get; set; } = null;

        public string[] DealsData
        {
            get
            {
                if (Deals is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<string[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }

        [JsonPropertyName("status")]
        public string StatusStr { get; set; } = string.Empty;

        [JsonIgnore]
        public OrderStatus StatusEnum => StatusStr.ToOrderStatus();
    }
}
