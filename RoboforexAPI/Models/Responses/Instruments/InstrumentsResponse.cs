using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Instruments
{
    public class InstrumentsResponse : BaseResponse
    {
        public InstrumentShort[] Instruments
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<InstrumentShort[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }
    }
}
