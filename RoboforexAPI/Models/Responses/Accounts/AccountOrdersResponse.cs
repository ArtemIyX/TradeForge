using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class AccountOrdersResponse : BaseResponse
    {
        public Entities.Order[] Orders
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<Entities.Order[]>(jsonElement.GetRawText()) ?? [];
                }
                return [];
            }
        }
    }
}
