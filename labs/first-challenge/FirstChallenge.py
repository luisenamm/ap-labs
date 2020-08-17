def get_length(lst):
    elements = 0
    for i in lst:
        if type(i) == list:  
            elements += get_length(i)
        else:
            elements += 1    
    return elements

lst=[1, [2, [3, [4, [5, 6]]]]]
elements = get_length(lst)
print (elements)