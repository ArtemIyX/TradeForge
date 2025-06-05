using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Entities
{
    public class AccountState
    {
        public object? AccountCashStateRaw { get; set; } = null;
        public object? AccountMarginStateRaw { get; set; } = null;

        public AccountCashState CashState
        {
            get
            {
                if (AccountCashStateRaw is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<AccountCashState>(jsonElement.GetRawText()) ?? new AccountCashState();
                }
                return new AccountCashState();
            }
        }

        public AccountMarginState MarginState
        {
            get
            {
                if (AccountMarginStateRaw is JsonElement jsonElement && jsonElement.ValueKind == JsonValueKind.Object)
                {
                    return JsonSerializer.Deserialize<AccountMarginState>(jsonElement.GetRawText()) ?? new AccountMarginState();
                }
                return new AccountMarginState();
            }
        }
    }
}
