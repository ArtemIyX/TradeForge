using RoboforexAPI.Models.Entities;
using System.Text.Json;

namespace RoboforexAPI.Models.Responses.Instruments
{
    public class InstrumentsResponse : BaseResponse
    {
        public InstrumentsResponse() { }

        public InstrumentsResponse(BaseResponse copy) : base(copy) { }

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
