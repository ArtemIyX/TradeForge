using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RoboforexAPI.Models.Requests
{
    public class AccountTickerRequest : BaseAccountRequest
    {
        public string Ticker { get; set; } = string.Empty;
    }
}
