function holder() {
    this.elements     = [];
    this.add_elements = [];
    this.del_elements = [];
    this.in_for_each  = false;
    
    this.for_each = function (functor) {
        this.in_for_each = true;
        for (var counter = 0; counter < this.elements.length; ++counter) {
            functor(this.elements[counter]);
        }
        this.in_for_each = false;
        this.commit();
    }
    
    this.commit = function () {
        for (var del_counter = 0; del_counter < this.del_elements.length; ++del_counter) {
            var del_number = this.elements.indexOf(this.del_elements[del_counter]);
            if (del_number !== -1) {
                this.elements.splice(del_number, 1);
            }
        }
        this.del_elements = [];
        
        for (var add_counter = 0; add_counter < this.add_elements.length; ++add_counter) {
            this.elements.push(this.add_elements[add_counter]);
        }
        this.add_elements = [];
    }
    
    this.add = function (element) {
        if (this.in_for_each) {
            this.add_elements.push(element);
        } else {
            this.elements.push(element);
        }
    }
    
    this.del = function (element) {
        if (this.in_for_each) {
            this.del_elements.push(element);
        } else {
            var del_number = this.elements.indexOf(element);
            if (del_number !== -1) {
                this.elements.splice(del_number, 1);
            }
        }
    }
}

exports.holder = holder;