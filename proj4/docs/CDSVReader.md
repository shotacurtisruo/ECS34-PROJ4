Markdown on CDSVReader:

    CDSVReader is meant to read DSV data from the source CDataSource using a delimiter 
    that's specified.

Methods:

    ReadRow(row): Reads a row from a DSV file and separates it into fields based 
    on the given delimiter. If the field has the delimiter, double quote, or newline, it 
    must be double quoted. Double quoted characters in the field are replaced with two. 
    If the row is successfully read, it returns true. 

    End(): returns true if the end of the DSV file has been reached (all rows have been read)