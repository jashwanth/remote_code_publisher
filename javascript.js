 function expand_collapse(id1, id2) {
  
   if (document.getElementById(id1).style.display == 'none') {
      document.getElementById(id1).style.display = 'block';

      document.getElementById(id2).value = "^";

      return false;

   }

   if (document.getElementById(id1).style.display == 'block') {

      document.getElementById(id1).style.display = 'none';

      document.getElementById(id2).value = "+";

      return false;
            
    }
        
 }

 function myalert(id) {
    alert(id);

 }