using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class DealsResponse : BaseResponse
    {
        public Entities.Deal[] Deals
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<Entities.Deal[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }
    }
}
