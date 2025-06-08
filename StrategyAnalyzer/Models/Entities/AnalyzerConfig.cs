using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StrategyAnalyzer.Models.Entities
{
    public class AnalyzerConfig
    {
        public RoboforexAPI.Models.Entities.InstrumentShort InstrumentShortDescription { get; set; } = new InstrumentShort();
        public RoboforexAPI.Models.Entities.Instrument InstrumentFullDescription { get; set; } = new Instrument();
    }
}
