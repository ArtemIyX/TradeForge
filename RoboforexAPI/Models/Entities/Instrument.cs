using RoboforexAPI.Models.Enums;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class Instrument
    {
        [JsonPropertyName("min_volume")]
        public double MinVolume { get; set; } = 0.0;

        [JsonPropertyName("max_volume")]
        public double MaxVolume { get; set; } = 0.0;

        [JsonPropertyName("volume_step")]
        public double VolumeStep { get; set; } = 0.0;

        [JsonPropertyName("min_tick")]
        public double MinTick { get; set; } = 0.0;

        [JsonPropertyName("leverage")]
        public double Leverage { get; set; } = 0.0;

        [JsonPropertyName("trade_mode")]
        public string TradeModeStr { get; set; } = string.Empty;

        public TradeMode TradeMode => TradeModeStr.ToTradeMode();
    }

    
}
