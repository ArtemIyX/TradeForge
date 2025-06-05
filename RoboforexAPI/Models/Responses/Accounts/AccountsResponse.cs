using RoboforexAPI.Models.Entities;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Responses.Accounts
{
    public class AccountsResponse : BaseResponse
    {
        public Entities.Account[] Accounts
        {
            get
            {
                if (Data is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Array)
                {
                    return JsonSerializer.Deserialize<Entities.Account[]>(jsonElement.GetRawText()) ?? Array.Empty<Entities.Account>();
                }
                return Array.Empty<Entities.Account>();
            }
        }
    }
}
