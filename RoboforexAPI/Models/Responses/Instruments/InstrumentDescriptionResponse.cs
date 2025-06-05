using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Instruments
{
    public class InstrumentDescriptionResponse : BaseResponse
    {
        public InstrumentDescriptionResponse() { }

        public InstrumentDescriptionResponse(BaseResponse copy) : base(copy) { }

        public Instrument InstrumentData
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<Instrument>(jsonElement.GetRawText()) ?? new Instrument();
                }
                return new Instrument();
            }
        }
    }
}
