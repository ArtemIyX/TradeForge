using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;


namespace RoboforexAPI.Models.Responses.Instruments
{
    public class QuoteDataResponse : BaseResponse
    {
        public QuoteDataResponse() { }

        public QuoteDataResponse(BaseResponse copy) : base(copy) { }

        public QuoteData Quote
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<QuoteData>(jsonElement.GetRawText()) ?? new QuoteData();
                }
                return new QuoteData();
            }
        }
    }
}
