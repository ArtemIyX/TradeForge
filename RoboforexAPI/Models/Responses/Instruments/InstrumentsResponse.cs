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
        public InsrumentShort[] Instruments
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<InsrumentShort[]>(jsonElement.GetRawText()) ?? Array.Empty<InsrumentShort>();
                }
                return Array.Empty<InsrumentShort>();
            }
        }
    }
}
