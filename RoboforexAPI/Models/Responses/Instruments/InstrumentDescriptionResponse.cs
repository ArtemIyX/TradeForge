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
        public Insrument InstrumentData
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<Insrument>(jsonElement.GetRawText()) ?? new Insrument();
                }
                return new Insrument();
            }
        }
    }
}
