Markdown on CDSVWriter:

    CDSVWriter is used to write given DSV data into a sink (data destination) using 
    a specified delimiter. 

Methods:
    
    WriteRow(row): This function takes a row as input and writes its fields into 
    the sink (CDataSink). If the field needs quotes, it's quoted. Quotes in a field are 
    doubled. 