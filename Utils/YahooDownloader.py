import sys
import subprocess
import importlib.util
import pandas as pd
from datetime import datetime
import os
import errno

def eprint(*args, **kwargs):
    """Print to stderr."""
    print(*args, file=sys.stderr, **kwargs)

def check_and_install_dependencies():
    """Check for required packages and install them if missing."""
    dependencies = ['yfinance', 'pandas']
    missing = []

    for dep in dependencies:
        if importlib.util.find_spec(dep) is None:
            missing.append(dep)

    if missing:
        eprint(f"Missing dependencies: {', '.join(missing)}. Installing...")
        try:
            subprocess.check_call([sys.executable, "-m", "pip", "install"] + missing)
            eprint("Dependencies installed successfully.")
        except subprocess.CalledProcessError as e:
            eprint(f"Error installing dependencies: {e}")
            return False
    return True

def download_stock_data(symbol, start_date, end_date, output_file):
    """Download stock data from Yahoo Finance and save to CSV."""
    try:
        import yfinance as yf

        eprint(f"Downloading data for symbol: {symbol}, start: {start_date}, end: {end_date}, output: {output_file}")

        # Validate output path
        output_dir = os.path.dirname(output_file)
        if output_dir and not os.path.exists(output_dir):
            eprint(f"Output directory does not exist: {output_dir}. Attempting to create...")
            try:
                os.makedirs(output_dir, exist_ok=True)
            except OSError as e:
                eprint(f"Error creating output directory {output_dir}: {e}")
                return 1

        # Check if output path is writable
        try:
            with open(output_file, 'a'):
                os.utime(output_file, None)  # Update file timestamp to test writability
        except OSError as e:
            if e.errno == errno.EACCES:
                eprint(f"Error: Permission denied to write to {output_file}")
            else:
                eprint(f"Error: Unable to access output file {output_file}: {e}")
            return 1

        # Validate date formats
        try:
            start = datetime.strptime(start_date, "%Y-%m-%d").date()
            end = datetime.strptime(end_date, "%Y-%m-%d").date()
            if start > end:
                eprint("Error: Start date must be before end date.")
                return 1
        except ValueError as e:
            eprint(f"Error: Invalid date format. Use YYYY-MM-DD. Details: {e}")
            return 1

        # Download data using yfinance
        stock = yf.Ticker(symbol)
        df = stock.history(start=start, end=end, interval="1d")

        if df.empty:
            eprint(f"Error: No data found for symbol {symbol} in the specified date range.")
            return 1

        # Save to CSV
        df.to_csv(output_file, index=True)
        eprint(f"Data successfully saved to {output_file}")
        return 0
    except Exception as e:
        eprint(f"Error in download_stock_data: {str(e)}")
        return 1

if __name__ == "__main__":
    eprint("Starting script...")
    # Expecting arguments: symbol, start_date, end_date, output_file
    if len(sys.argv) != 5:
        eprint(f"Usage: python {sys.argv[0]} <symbol> <start_date> <end_date> <output_file>")
        sys.exit(1)

    symbol = sys.argv[1]
    start_date = sys.argv[2]
    end_date = sys.argv[3]
    output_file = sys.argv[4]

    eprint(f"Received arguments: symbol={symbol}, start_date={start_date}, end_date={end_date}, output_file={output_file}")

    # Check and install dependencies
    if not check_and_install_dependencies():
        eprint("Failed to install dependencies.")
        sys.exit(1)

    # Download and save data
    result = download_stock_data(symbol, start_date, end_date, output_file)
    eprint(f"Script completed with exit code: {result}")
    sys.exit(result)